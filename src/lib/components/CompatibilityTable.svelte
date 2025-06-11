<script lang="ts">
	import type { DeviceCompatibility, FeatureRow } from '$lib/types/device';
	import devicesData from '$lib/data/devices.json';

	const compatibilityData: DeviceCompatibility[] = devicesData;

	// Get features dynamically from the first object
	let features: (keyof FeatureRow)[] = [];
	if (compatibilityData.length > 0) {
		features = Object.keys(compatibilityData[0]).filter((k) => k !== 'device') as (keyof FeatureRow)[];
	}
</script>

<div class="container">
	<div class="mx-auto mt-10 w-[90%] text-center">
		<h1 class="mb-5 text-3xl font-bold text-white">Compatible Devices</h1>
		<div class="w-full overflow-x-hidden">
			<table class="w-full border-collapse overflow-hidden rounded-lg bg-neutral-900">
				<thead>
					<tr>
						<th class="px-4 py-3 text-center text-base text-white">Device</th>
						{#each features as feat}
							<th class="px-4 py-3 text-center text-base text-white">{feat.replace('_', ' ')}</th>
						{/each}
					</tr>
				</thead>
				<tbody>
					{#each compatibilityData as row}
						<tr class="even:bg-neutral-800 hover:bg-neutral-700">
							<td class="border-b border-neutral-800 px-4 py-3 text-center text-base text-white">{row.device}</td>
							{#each features as key}
								<td class={row[key] === true ? 'success' : row[key] === false ? 'fail' : ''}>
									{#if key === 'Speaker' && typeof row[key] === 'string'}
										{row[key]}
									{:else}
										{row[key] === true ? '✅' : row[key] === false ? '❌' : ''}
									{/if}
								</td>
							{/each}
						</tr>
					{/each}
				</tbody>
			</table>
		</div>
		<br />
		<h2 class="text-lg">
			For <strong>Wiring Diagrams</strong> check the
			<a href="https://github.com/pr3y/Bruce/tree/main/media/connections"><b>connections</b></a>
			or <a href="https://github.com/pr3y/Bruce/wiki"><b>Wiki</b></a>!
		</h2>
		<h2 class="text-lg">Every feature is also listed on Github.</h2>
	</div>
</div>

<style>
	.container {
		width: 90%;
		max-width: 100%;
		margin: 0 auto;
	}
</style>
