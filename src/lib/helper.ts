export function uint8ArrayToString(uint8Array: Uint8Array) {
	const decoder = new TextDecoder('utf-8'); // Specify the encoding (default is 'utf-8')
	return decoder.decode(uint8Array);
}

export function capitalize(str: string) {
	if (!str) return str; // Handle empty or null strings
	return str.charAt(0).toUpperCase() + str.slice(1).toLowerCase();
}

export function bytesToMegabytes(bytes: number) {
	if (bytes < 0) {
		throw new Error('Input must be a non-negative number');
	}

	return (bytes / (1024 * 1024)).toFixed(2);
}

export function bytesToKB(bytes: number) {
	if (bytes < 0) {
		throw new Error('Input must be a non-negative number');
	}

	return (bytes / 1024).toFixed(2);
}
