<script>
	export let title;
</script>

<div
	class="m-2.5 max-w-[400px] rounded-[10px] bg-[#222] p-5 text-center transition-all duration-300 ease-linear hover:scale-105 max-sm:m-2 max-sm:h-100"
>
	<h3 class="mb-2.5 text-[1.8rem] font-bold">{title}</h3>
	<slot />
</div>
