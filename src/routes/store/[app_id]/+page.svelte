<script lang="ts">
	import type { PageProps } from './$types';
	import { current_page, Page } from '$lib/store';

	let { data }: PageProps = $props();
	const components = import.meta.glob('$lib/apps/*.md', { eager: true });

	$current_page = Page.AppStore;
</script>

<div class="mt-32">
	{#each Object.entries(components) as component}
		{#if component[0].includes(data.app_id)}
			{@const App = component[1].default}
			<App filename={component[0]} />
		{/if}
	{/each}
</div>
