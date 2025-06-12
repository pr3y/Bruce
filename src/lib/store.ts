export enum Page {
	Home,
	Flasher
}

import { writable } from 'svelte/store';
export const current_page = writable(Page.Home);
