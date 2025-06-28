<script>
	let props = $props();

	let isOpen = $state(false);

	function toggleDropdown() {
		isOpen = !isOpen;
	}

	function handleClickOutside(event) {
		if (!event.target.closest('.relative.inline-block')) {
			isOpen = false;
		}
	}
</script>

<svelte:window onclick={handleClickOutside} />

<div class="relative inline-block text-left">
	<button
		type="button"
		class="inline-flex w-full justify-center rounded-lg px-4 py-2.5 text-sm font-medium text-purple-500 shadow-sm focus:ring-2 focus:ring-indigo-500 focus:ring-offset-2 focus:outline-none"
		onclick={toggleDropdown}
	>
		{props.title}
		<svg class="-mr-1 ml-2 h-5 w-5" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 20 20" fill="currentColor" aria-hidden="true">
			<path
				fill-rule="evenodd"
				d="M5.293 7.293a1 1 0 011.414 0L10 10.586l3.293-3.293a1 1 0 111.414 1.414l-4 4a1 1 0 01-1.414 0l-4-4a1 1 0 010-1.414z"
				clip-rule="evenodd"
			/>
		</svg>
	</button>

	{#if isOpen}
		<div class="shaodw-md absolute right-0 mt-2 w-56 origin-top-right rounded-md bg-[#222] py-1 text-center shadow-lg focus:outline-none">
			{#each props.links as link}
				<a href={link.href}>{link.title}</a>
			{/each}
		</div>
	{/if}
</div>

<style>
</style>
