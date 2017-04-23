package VideoRsa;

public class SyncData {
	byte[] Data;

	synchronized void setData(byte[] Data) {
		this.Data = Data.clone();
	}

	synchronized byte[] getData() {
		return this.Data.clone();
	}
}
