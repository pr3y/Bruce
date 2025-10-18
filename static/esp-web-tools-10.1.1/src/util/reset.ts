import { Transport } from "esptool-js";
import { sleep } from "./sleep";

export const hardReset = async (transport: Transport) => {
  console.log("Triggering reset");
  await transport.device.setSignals({
    dataTerminalReady: false,
    requestToSend: true,
  });
  await sleep(250);
  await transport.device.setSignals({
    dataTerminalReady: false,
    requestToSend: false,
  });
  await sleep(250);
  await new Promise((resolve) => setTimeout(resolve, 1000));
};
