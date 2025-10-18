/**
 * Represents a chip ROM with basic registers field and abstract functions.
 */
class ROM {
  /**
   * Get the chip erase size.
   * @param {number} offset - Offset to start erase.
   * @param {number} size - Size to erase.
   * @returns {number} The erase size of the chip as number.
   */
  getEraseSize(offset, size) {
    return size;
  }
}

export { ROM as R };
