package VideoRsa;

import java.io.*;
import java.net.*;
//import java.util.Random;
import javax.imageio.ImageIO;
import javax.swing.*;
import java.awt.image.BufferedImage;

class ClientClass extends Thread {
	DataOutputStream output;
	Socket socket = null;
	DataInputStream input = null;

	public void run() {

		try {
			socket = new Socket("192.168.20.246", 5005);
			input = new DataInputStream(socket.getInputStream());
			System.out.println("Cross1 ");
		} catch (UnknownHostException e1) {

			e1.printStackTrace();
		} catch (IOException e1) {

			e1.printStackTrace();
		}
		try {

			InputStreamReader isr = new InputStreamReader(System.in);
			BufferedReader br = new BufferedReader(isr);
			output = new DataOutputStream(socket.getOutputStream());

			int e = input.readInt();
			int n = input.readInt();

			strRes strres1 = new strRes();
			XORGen XOR = new XORGen();
			RSAProc RSA = new RSAProc();
			int xor = XOR.getXorKey();
			//byte[] xorBytes = XOR.getXorKeyBytes();
			output.writeInt(RSA.RSAencr(xor, n, e));

			System.out.println("Choose resolution --> 1 for 320x180/n 2 for 480x270/n 3 for 640x360/n 4 for 800x450/n:");
			String res = br.readLine();
			output.writeInt(RSA.RSAencr(Integer.parseInt(res), n, e));
			String stringRES = strres1.strres(Integer.parseInt(res));
			
			System.out.println("Give fps 25-30: /n");
			String fps = br.readLine();
			System.out.println("fps :  " + fps);
			output.writeInt(RSA.RSAencr(Integer.parseInt(fps), n, e));
			
			JFrame imageGUI = new JFrame();
			imageGUI.setVisible(true);
			imageGUI.setSize(850, 500);
			imageGUI.setTitle("Our beautiful Video with Resolution: " + stringRES + " and FPS: " + fps);
			imageGUI.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
			final JLabel label = new JLabel();
			imageGUI.add(label);

			while (true) {
				final SyncData SD = new SyncData();
				if (socket.isConnected()) {
					System.out.println("Socket is Connected");
					int encrpicsize = input.readInt();
					//System.out.println("Receive encrypted picsize: " + encrpicsize);
					int picsize = encrpicsize ^ xor;
					//System.out.println("Picture size: " + picsize);
					byte[] img = new byte[picsize];

					int x = 0;
					//System.out.println("try to read image");
					while (true) {
						int read = input.read(img, x, picsize - x);
						x = x + read;
						if (x == picsize)
							break;
					}
					System.out.println("image read");
					byte[] decryptIMG = XOR.xorDecrypt(img);
					SD.setData(decryptIMG);

					SwingUtilities.invokeLater(new Runnable() {
						public void run() {
							InputStream in = new ByteArrayInputStream(SD.getData());
							BufferedImage bImg = null;
							try {
								bImg = ImageIO.read(in);
							} catch (IOException e) {
								e.printStackTrace();
							}
							ImageIcon image = new ImageIcon(bImg);
							label.setIcon(image);
							label.repaint();
							label.validate();
						}
					});
				}
			}

		} catch (IOException e) {
			System.out.println("Error 2... ");
		}

	}

}

public class Client1 {

	public static void main(String args[]) throws UnknownHostException, IOException {

		Thread myClass = new ClientClass();
		myClass.start();

	}

}





















