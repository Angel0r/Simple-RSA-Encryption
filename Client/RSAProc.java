package VideoRsa;

public class RSAProc {
	
	public int RSAencr(int m, int n, int e) {
	        int j = 0;
	        long encryptedM = 1;
	        for (j = 0; j < e; j++) {
	            encryptedM = encryptedM * m % n;
	        }
	        return (int) encryptedM;
	    }
}
/*public int RSAencr(int m, int n, int e) {

long en = (long) Math.pow(m, e);
long encrm = en % n;
return (int) encrm;
}*/