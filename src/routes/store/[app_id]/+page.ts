import type { PageLoad } from './$types';

export const load: PageLoad = ({ params }) => {
	return {
		app_id: params.app_id
	};
};
