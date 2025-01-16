function main() {
  // gifOpen - Returns a handle for the GIF. If it returns 0, it means there was an error opening the file.
  const patrick = gifOpen('littlefs', '/gifs/half_patrick.gif');
  const spongebob = gifOpen('littlefs', '/gifs/half_spongebob.gif');
  const rick = gifOpen('littlefs', '/gifs/half_rick.gif');
  const rainbow = gifOpen('littlefs', '/gifs/half_rainbow.gif');

  // Check if all GIFs failed to load
  if (!patrick && !spongebob && !rick && !rainbow) {
    println("Cannot load gifs");
    println("Make sure that they are in /gifs folder");
    delay(3000);
    throw new Error("Cannot load gifs");
  }

  while(true) {
    gifPlayFrame(patrick, 0, 0); // Plays a frame of the GIF at the specified position.
    gifPlayFrame(spongebob, 60, 0);
    gifPlayFrame(rick, 120, 0);
    gifPlayFrame(rainbow, 180, 0);

    // To remove gif you can use:
    // const dimensions = gifDimensions(rainbow); // Retrieves the dimensions of the Rainbow GIF.
    // drawFillRect(x + 120, y, dimensions.width, dimensions.height, color(0, 0, 0)); // Clears the previous frame.
    // gifClose(rainbow, x + 120, y); // Closes the Rainbow GIF to release resources.

    // gifPlayFrame(patrick, x, y);
    // You can move GIFs by changing x and y. Ensure you clear the old position
    // with drawFillRect(x, y, dimensions.width, dimensions.height, color(0, 0, 0))
    // and gifReset(patrick) it because GIFs are often optimized to draw only the pixels that change between frames.
  
    if (getAnyPress()) {
      break; // Exits the loop when a button is pressed.
    }
  }
}

main();