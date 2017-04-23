package VideoRsa;

/*import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;*/

public class strRes {

	public String strres(int re) {
		if (re == 1) {
			return "320x180";
		} else if (re == 2) {
			return "480x270";
		} else if (re == 3) {
			return "640x360";
		} else if (re == 4) {
			return "800x450";

		}
		return "320x180";
	}

	/*public static void main(String[] args) throws IOException {
		InputStreamReader isr = new InputStreamReader(System.in);
		BufferedReader br = new BufferedReader(isr);
		System.out.println("Choose resolution --> 1 for 320x180, 2 for 480x270, 3 for 640x360, 4 for 800x450:");
		String res = br.readLine();
		String stringRES = strres(Integer.parseInt(res));
		System.out.println("The choosen resolution is: " + stringRES);
	}*/
}
