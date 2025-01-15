const patrick = gifOpen('littlefs', '/gifs/half_patrick.gif');
const spongebob = gifOpen('littlefs', '/gifs/half_spongebob.gif');
const rick = gifOpen('littlefs', '/gifs/half_rick.gif');
const rainbow = gifOpen('littlefs', '/gifs/half_rainbow.gif');

while(true) {
  gifPlayFrame(patrick, 0, 0);
  gifPlayFrame(spongebob, 60, 0);
  gifPlayFrame(rick, 120, 0);
  gifPlayFrame(rainbow, 180, 0);

  if (getPrevPress()) {
    break;
  }
}
