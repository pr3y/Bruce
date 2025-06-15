export enum Page {
	Home,
	Flasher,
	Donate,
	MyBruce
}

import { writable } from 'svelte/store';
export const current_page = writable(Page.Home);
