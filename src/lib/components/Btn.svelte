<script lang="ts">
	export let href: string | undefined = undefined;
	export let className: string = '';
	export let outline: boolean = false;
</script>

{#if !outline}
	<a
		{href}
		class="inline-block rounded bg-[#9B51E0] px-6 py-3 font-semibold text-white transition-all duration-300 ease-in-out hover:scale-105 hover:bg-[#8033C7] {className}"
		{...$$restProps}
		style="color:white"
	>
		<slot />
	</a>
{:else}
	<a
		{href}
		class="inline-block rounded-lg border-2 border-[#9B51E0] bg-transparent px-6 py-3 font-semibold text-[#9B51E0] transition-all duration-300 ease-in-out hover:scale-105 hover:bg-[#9B51E0] hover:text-white {className}"
		{...$$restProps}
		style="color:white"
	>
		<slot />
	</a>
{/if}
