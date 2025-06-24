<script lang="ts">
	import { base } from '$app/paths';
	import { capitalize } from '$lib/helper';
	import { current_page, Page } from '$lib/store';
	import categories from '$lib/data/categories.json';

	const components = import.meta.glob('$lib/apps/*.md', { eager: true });
	console.log(Object.entries(components));

	$current_page = Page.AppStore;

	let applications = $state(Object.entries(components));

	function filter(cat: string) {
		if (current_cat == cat) {
			// Reset the state
			applications = Object.entries(components);
			current_cat = '';
		} else {
			applications = Object.entries(components).filter((val) => {
				return val[1].metadata.category == cat;
			});
			current_cat = cat;
		}
	}

	let current_cat = '';
</script>

<div class="mt-32 text-center">
	<h2 class="m-10 text-2xl font-bold">Contribute with your theme or your script using the forum or the Discord channels!</h2>
	<h2 class="m-10 text-2xl font-bold underline">
		<a href="https://forum.bruce.computer/t/1st-bruce-theme-contest/35" target="_blank">Take part in Bruce's theme contest!</a>
	</h2>
	{#each categories as category}
		<button onclick={() => filter(category.name)}>
			<div
				class="inline-flex items-center gap-3 {category.color} m-1 rounded-full px-6 py-3 text-white transition-transform duration-300 ease-in-out hover:scale-105 hover:shadow-2xl"
			>
				<span class="h-6 w-6">
					{category.emoji}
				</span>

				<!-- Text -->
				<span class="text-lg font-semibold tracking-wide">{capitalize(category.name)}</span>
			</div>
		</button>
	{/each}

	<div class="grid grid-cols-3">
		{#each applications as element}
			{@const CardApp = element[1].default}
			<a href="{base}/store/{element[1].metadata.id}">
				<CardApp card href="" />
			</a>
		{/each}
	</div>
</div>
