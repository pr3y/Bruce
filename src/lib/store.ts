export enum Page {
	Home
}

import { writable } from 'svelte/store';
export const current_page = writable(Page.Home);
