import { uint8ArrayToString } from './helper';

export async function requestSerialPort(baud_rate: number): Promise<SerialPort> {
	let port: SerialPort = await navigator.serial.requestPort();
	await port.open({ baudRate: baud_rate });

	return port;
}

export async function readSerialPort(port: SerialPort, callback: Function) {
	return new Promise(async (resolve, reject) => {
		const reader = port.readable.getReader();

		try {
			setTimeout(async () => {
				while (true) {
					const res = await reader.read();
					if (res.value) {
						callback(uint8ArrayToString(res.value));
						//reader.releaseLock();
						await reader.cancel(); // Stop reader to get values when finish
						// break;
						resolve(null);
						break;
					}
				}
			}, 5000);
		} catch (error) {
			alert('Error during communication with device');
		}
	});
}

export async function writeToPort(port: SerialPort, data: Uint8Array<ArrayBufferLike>) {
	if (!port) return;
	const writer = port.writable.getWriter();
	try {
		await writer.write(data);
	} catch (error) {
		alert('Error during communication with device');
		console.error(error);
	} finally {
		writer.releaseLock();
	}
}
