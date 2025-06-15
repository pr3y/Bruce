<script lang="ts">
	import { base } from '$app/paths';
	import Btn from '$lib/components/Btn.svelte';
	import InfoRow from '$lib/components/InfoRow.svelte';
	import { current_page, Page } from '$lib/store';
	import { bytesToKB, bytesToMegabytes, capitalize } from '$lib/helper';
	import { readSerialPort, requestSerialPort, writeToPort } from '$lib/serial_helper';
	$current_page = Page.MyBruce;

	let port: SerialPort;
	const baud_rate = 115200;
	let connected = $state(false);
	const supported = 'serial' in navigator;

	let sdCard = $state('');
	let littleFS_Storage = $state('');
	let device = $state('');
	let version = $state('');
	let sdk = $state('');
	let mac_address = $state('');
	let wifi = $state('');
	let uptime = $state('');
	let heap_usage = $state('');
	let psram_usage = $state('');

	let loading = $state(false);

	function parseDeviceInfo(response: string) {
		const lines = response.split('\n');
		console.log(lines);
		version = `${lines[0]} - ${lines[1]}`;
		sdk = lines[2].replace('SDK: ', '');
		mac_address = lines[3].replace('MAC addr:', '');
		wifi = capitalize(lines[4].replace('Wifi: ', ''));
		device = lines[5].replace('Device: ', '');
	}

	function parseUptime(response: string) {
		const lines = response.split('\n');
		uptime = lines[0].replace('Uptime: ', '');
	}

	function parseFree(response: string) {
		const lines = response.split('\n');
		if (lines.length >= 3) {
			// replace method extract the numbers
			let total_heap = +lines[0].replace(/\D/g, '');
			let free_heap = +lines[1].replace(/\D/g, '');
			heap_usage = `${bytesToKB(total_heap - free_heap)}/${bytesToKB(total_heap)} KB`;

			if (lines.length == 5) {
				let total_psram = +lines[2].replace(/\D/g, '');
				let free_psram = +lines[3].replace(/\D/g, '');
				psram_usage = `${bytesToMegabytes(total_psram - free_psram)}/${bytesToMegabytes(free_psram)} MB`;
			}
		}
	}

	function prepare_storage_string(str: string) {
		return str.replace('Bytes', '');
	}

	function parseFreeStorage(response: string) {
		console.log(response);
		if (response.includes('[E][sd_diskio.cpp')) {
			sdCard = 'Not Installed';
		} else if (response.includes('SD Total space')) {
			const lines = response.split('\n');
			sdCard = `${bytesToMegabytes(+prepare_storage_string(lines[1].replace('SD Used space: ', '')))} / ${bytesToMegabytes(+prepare_storage_string(lines[0].replace('SD Total space: ', '')))} MB`;
		} else if (response.includes('LittleFS Total space')) {
			const lines = response.split('\n');
			littleFS_Storage = `${bytesToMegabytes(+prepare_storage_string(lines[1].replace('LittleFS Used space: ', '')))} / ${bytesToMegabytes(+prepare_storage_string(lines[0].replace('LittleFS Total space: ', '')))} MB`;
		}
	}

	async function get_info() {
		await writeToPort(port, new TextEncoder().encode('info'));
	}

	async function get_uptime() {
		await writeToPort(port, new TextEncoder().encode('uptime'));
	}

	async function get_free() {
		await writeToPort(port, new TextEncoder().encode('free'));
	}

	async function get_sd_store() {
		await writeToPort(port, new TextEncoder().encode('storage free sd'));
	}

	async function get_littlefs_usage() {
		await writeToPort(port, new TextEncoder().encode('storage free littlefs'));
	}

	async function connect_device() {
		try {
			loading = true;
			port = await requestSerialPort(baud_rate);
			if (port) {
				connected = true;

				await get_sd_store();
				await readSerialPort(port, parseFreeStorage);

				await get_littlefs_usage();
				await readSerialPort(port, parseFreeStorage);

				await get_info();
				await readSerialPort(port, parseDeviceInfo);

				await get_uptime();
				await readSerialPort(port, parseUptime);

				await get_free();
				await readSerialPort(port, parseFree);

				get_device_image();
			}
			loading = false;
		} catch (error) {
			console.error(error);
			alert('Some error occured during communication with device');
		}
	}

	async function factory_reset() {
		if (confirm('Are you sure? Factory reset will reset the content of Bruce.conf')) {
			await writeToPort(port, new TextEncoder().encode('factory_reset'));
		}
	}

	async function reboot_bruce() {
		if (confirm('Are you sure?')) {
			await writeToPort(port, new TextEncoder().encode('power reboot'));
		}
	}

	const devices = [
		{
			name: 'M5StickC',
			img: 'm5stick.png'
		},
		{
			name: 'M5Stack Core',
			img: 'core2.png'
		},
		{
			name: 'Cardputer',
			img: 'cardputer.png'
		},
		{
			name: 'Lilygo T-Embed',
			img: 't-embed.png'
		},
		{
			name: 'CYD',
			img: 'cyd.png'
		},
		{
			name: 'Phantom',
			img: 'cyd.png'
		},
		{
			name: 'Smoochiee Board',
			img: 'bruce-pcb.png'
		}
	];

	let img = $state('');
	function get_device_image() {
		let _img = devices.find((_name) => device.includes(_name.name));
		if (_img != null) {
			img = _img.img;
		} else {
			img = 'bruce-logo.png';
		}
	}
</script>

{#if !supported}
	<h1 class="mt-32 text-center text-3xl font-bold">Unsupported browser</h1>
	<h1 class="mt-5 mb-10 text-center text-3xl font-bold">Please use a Chromium based browser</h1>
{:else if !connected}
	<div class="flex items-center justify-center text-center">
		<Btn className="mt-32 mb-10" onclick={connect_device}>Connect</Btn>
	</div>
{:else if loading}
	<div class="mt-32 flex flex-col items-center justify-center">
		<div class="h-16 w-16 animate-spin rounded-full border-4 border-blue-600 border-t-transparent" role="status"></div>

		<p class="mt-5 mb-5 text-center text-white" style="color:white;">Loading... This may take a few seconds</p>
	</div>
{:else}
	<div class="mx-auto mt-32 max-w-5xl rounded-lg">
		<div class="flex items-center justify-between">
			<div class="flex-1">
				<div class="space-y-3">
					<InfoRow label="Firmware" value={version} />
					<InfoRow label="SD card" value={sdCard} />
					<InfoRow label="LittleFS" value={littleFS_Storage} />
					<InfoRow label="Hardware" value={device} />
					<InfoRow label="MAC Address" value={mac_address} />
					<InfoRow label="WiFi" value={wifi} />
					<InfoRow label="Heap usage" value={heap_usage} />
					<InfoRow label="PSRAM usage" value={psram_usage} />
					<!-- <InfoRow label="Total heap" value={total_heap} />
					<InfoRow label="Free heap" value={free_heap} />
					<InfoRow label="Total PSRAM" value={total_psram} />
					<InfoRow label="Free PSRAM" value={free_psram} /> -->
					<InfoRow label="Uptime" value={uptime} />
					<InfoRow label="SDK" value={sdk} />
				</div>
			</div>

			<div class="flex-shrink-0">
				<img src="{base}/img/{img}" alt="Bruce device" />
			</div>
		</div>
		<div class="mt-10 mb-10">
			<Btn href="{base}/flasher">Update</Btn>
			<Btn onclick={factory_reset}>Factory Reset</Btn>
			<Btn onclick={reboot_bruce}>Reboot</Btn>
		</div>
	</div>
{/if}
