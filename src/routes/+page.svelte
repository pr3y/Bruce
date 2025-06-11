<script lang="ts">
	import { onMount } from 'svelte';
	const VERSION = '1.10.2';

	import Card from '$lib/Card.svelte';
	import News from '$lib/News.svelte';
	import Btn from '$lib/Btn.svelte';
	import CompatibilityTable from '$lib/components/CompatibilityTable.svelte';

	let activeIndex = 0;
	let slides = [
		'/img/bruce-pcb.png',
		'/img/cardputer.png',
		'/img/core2.png',
		'/img/cyd.png',
		'/img/lilygo.png',
		'/img/t-embed.png',
		'/img/m5stick.png'
	];

	let interval: number;
	onMount(() => {
		interval = setInterval(() => {
			activeIndex = (activeIndex + 1) % slides.length;
		}, 3000);
		return () => clearInterval(interval);
	});

	function setSlide(idx: number) {
		activeIndex = idx;
	}
</script>

<!-- Hero Section -->
<section class="relative flex h-[500px] w-full flex-col overflow-hidden pr-4 pl-4 md:flex-row">
	<img src="/img/purple-aqua.gif" alt="Background GIF" class="absolute top-0 left-0 h-full w-full object-cover brightness-75" />
	<div class="relative z-10 flex flex-col justify-center p-8 text-white">
		<h1 class="mb-5 text-4xl font-bold md:text-6xl">Bruce Firmware</h1>
		<p class="mb-7 text-xl">The powerful open-source ESP32 firmware designed for offensive security and Red Team operations.</p>
		<div class="flex gap-4">
			<a
				href="https://github.com/pr3y/Bruce"
				target="_blank"
				class="inline-blocktransition rounded-lg bg-[#9B51E0] px-6 py-2 text-lg font-bold text-white"
				style="color: white;"
			>
				Explore GitHub
			</a>
			<a
				href="https://bruce.computer/flasher"
				target="_blank"
				class="ml-4 inline-block rounded-lg border-2 border-[#9B51E0] px-6 py-2 text-lg text-[#9B51E0] transition"
			>
				<b>Install Now</b>
			</a>
		</div>
	</div>
	<div class="w-300">
		{#each slides as slide, i}
			<div class="slide {i === activeIndex ? 'active' : ''}">
				<img class="max-w" src={slide} alt={'Slide ' + (i + 1)} />
			</div>
		{/each}
		<div class="dot-nav">
			{#each slides as _, i}
				<span class="dot {i === activeIndex ? 'active' : ''}" onclick={() => setSlide(i)}></span>
			{/each}
		</div>
	</div>
</section>

<!-- Features -->
<div class="p-10 text-center">
	<div class="container">
		<h1 class="mb-5 text-3xl font-bold">Why Bruce?</h1>
		<div class="flex justify-center">
			<div class="grid grid-cols-1 gap-4 md:grid-cols-2 lg:grid-cols-3">
				<Card>
					<h3 class="mb-3 text-lg font-bold">⚡ True Open-Source</h3>
					<p>
						Bruce fw is licensed under <b><a href="https://www.gnu.org/licenses/agpl-3.0.en.html#license-text" target="_blank">AGPL</a></b>, and
						Hardware is
						<b
							><a href="https://ohwr.org/project/cernohl/-/wikis/uploads/3eff4154d05e7a0459f3ddbf0674cae4/cern_ohl_p_v2.txt" target="_blank"
								>CERN-OHL-P-2.0</a
							></b
						>. Free as in freedom.
					</p>
				</Card>
				<Card class="border-none bg-[#222]" size="xs">
					<div class="p-4">
						<h3 class="mt-3 mb-3 text-lg font-bold">🖥️ Cross-Platform</h3>
						<p>Bruce runs seamlessly on M5Stack, LilyGo, and other ESP32-based devices.</p>
					</div>
				</Card>
				<Card class="border-none bg-[#222]" size="xs">
					<div class="p-4">
						<h3 class="mt-3 mb-3 text-lg font-bold">📶 Wi-Fi Attacks</h3>
						<p>
							Supports Evil Portal, Wardriving, EAPOL handshake capture, Deauth
							<b><a href="https://github.com/pr3y/bruce/?tab=readme-ov-file#wifi" target="_blank">and more</a></b>.
						</p>
					</div>
				</Card>
				<Card class="border-none bg-[#222]" size="xs">
					<div class="p-4">
						<h3 class="mt-3 mb-3 text-lg font-bold">📜 Documentation</h3>
						<p>The project has every information about the features and modules supported available on github.</p>
					</div>
				</Card>
				<Card class="border-none bg-[#222]" size="xs">
					<div class="p-4">
						<h3 class="mt-3 mb-3 text-lg font-bold">📡 SubGHz and RFID</h3>
						<p>Supports several modules and devices with frequency transceivers.</p>
					</div>
				</Card>
				<Card class="border-none bg-[#222]" size="xs">
					<div class="p-4">
						<h3 class="mt-3 mb-3 text-lg font-bold">🚀 Active Community</h3>
						<p>Regular updates and community-driven improvements.</p>
					</div>
				</Card>
			</div>
		</div>
	</div>
</div>

<!-- News -->

<div class="flex w-full items-center justify-center px-5 py-12 text-center">
	<div class="w-full max-w-4xl">
		<h1 class="mb-5 text-4xl font-bold">News</h1>
		<div class="flex items-center justify-center">
			<News title="Release v{VERSION}">
				<p class="mb-4 text-[1.2rem]">
					Our new Release is out now! Update your device
					<a href="https://bruce.computer/flasher" target="_blank"><b>now</b></a>
				</p>
				<Btn href="https://github.com/pr3y/Bruce/releases/tag/{VERSION}">Read Changelog</Btn>
			</News>
			<News title="Bruce PCB">
				<p class="mb-4 text-[1.2rem]">Open Source Bruce PCB, fully compatible with Bruce</p>
				<Btn href="https://github.com/pr3y/Bruce/tree/main/pcbs">Download</Btn>
				<Btn href="https://www.pcbway.com/project/shareproject/Bruce_PCB_Smoochiee_d6a0284b.html" outline>Buy</Btn>
			</News>
		</div>
	</div>
</div>

<CompatibilityTable></CompatibilityTable>

<!-- FAQ -->
<div class="flex w-full items-center justify-center px-5 py-12 text-center">
	<div>
		<h2 class="mb-5 text-2xl font-bold">Need more help?</h2>
		<p>
			Check out our <a href="https://github.com/pr3y/Bruce/wiki/FAQ" target="_blank" rel="noopener noreferrer"><b>FAQ</b></a>!
		</p>
	</div>
</div>

<style>
	.container {
		width: 90%;
		max-width: 100%;
		margin: 0 auto;
	}

	.slide {
		opacity: 0;
		transition: opacity 0.6s ease-in-out;
		position: absolute;
		top: 55%;
		left: 75%;
		transform: translate(-50%, -50%);
		width: 50%;
		z-index: 4;
	}

	.slide img {
		width: 140%;
		height: auto;
	}

	.slide.active {
		opacity: 1;
		z-index: 5;
	}

	.slide.active {
		opacity: 1;
		z-index: 5;
	}

	.slide img {
		width: 100%;
		height: auto;
		display: block;
	}
	.dot-nav {
		position: absolute;
		bottom: 20px;
		left: 75%;
		transform: translateX(-50%);
		z-index: 10;
		display: flex;
		gap: 10px;
	}

	.dot {
		width: 12px;
		height: 12px;
		border-radius: 50%;
		background-color: rgba(255, 255, 255, 0.5);
		cursor: pointer;
		transition: background-color 0.3s ease;
		position: relative;
		z-index: 10;
	}

	.dot.active {
		background-color: #9b51e0;
	}

	.dot:hover {
		background-color: #9b51e0;
	}
</style>
