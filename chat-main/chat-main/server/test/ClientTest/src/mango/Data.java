package mango;

public record Data(
		String name,
		byte lang,
		int code) {
	public Data(String name) {
		this.name=name;
	}
}
