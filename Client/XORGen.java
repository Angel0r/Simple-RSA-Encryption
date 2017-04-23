package VideoRsa;

import java.util.Random;

public class XORGen {
	private byte[] xorKeyBytes = new byte[2];
	private int xorKey;

	public XORGen() {
		Random rand = new Random();
		xorKey = rand.nextInt(55000) + 3000;
		System.out.println("Random XOR key: " + xorKey);
		xorKeyBytes[0] = (byte) (xorKey & 0xFF);
		xorKeyBytes[1] = (byte) ((xorKey >>> 8) & 0xFF);
		for (int i = 0; i < xorKeyBytes.length; i++)
			System.out.println("Random XOR keybytes: " + xorKeyBytes[0]);
		System.out.println("Random XOR keybytes1: " + xorKeyBytes[1]);
	}

	public byte[] xorDecrypt(byte[] data) {
		byte[] dataToReturn = new byte[data.length];
		for (int i = 0; i < data.length; i++) {
			int index = (xorKeyBytes.length - 1) - (i % xorKeyBytes.length);
			dataToReturn[i] = (byte) (data[i] ^ xorKeyBytes[index]);
		}
		return dataToReturn;
	}

	public byte[] getXorKeyBytes() {
		return xorKeyBytes;
	}

	public int getXorKey() {
		return xorKey;
	}
}
/*
 * public byte[] xorDecrypt(byte[] a, byte[] b) { byte[] result = new
 * byte[Math.min(a.length, b.length)];
 * 
 * for (int i = 0; i < result.length; i++) { result[i] = (byte) (((int) a[i]) ^
 * ((int) b[i])); }
 * 
 * return result; }
 */
