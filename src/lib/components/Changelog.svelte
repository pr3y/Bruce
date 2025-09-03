<script>
	let { children } = $props();
</script>

<h2 class="mt-10 mb-5 text-4xl font-bold">Changelog</h2>

{@render children?.()}

<svelte:head>
	<style>
        h4 {
            font-size: 1.5rem;
            font-weight: bold;
        }

        li {
            list-svelte:head-type: disc;
        }
        ul {
            list-style-type: disc;
            padding: 1rem;
        }
	</style>
</svelte:head>
