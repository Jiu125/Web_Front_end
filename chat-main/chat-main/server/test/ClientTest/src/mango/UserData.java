package mango;

/**
 * 사용자 데이터(친구 데이터 제외)는 앱 실행 중 딱 한 번만 생성되어야 한다.
 *  
 */
public class UserData {
	private String id;
	private String pwd;
	private String name;
	private byte lang;
	private int code;
	
	/**
	 * 로그인용 데이터를 만드는데 사용되는 생성자
	 * @param id
	 * @param pwd 암호화된 비밀번호
	 */
	public UserData(String id,String pwd) {
		this.id=id;
		this.pwd=pwd;
	}
	/**
	 * 회원가입 데이터를 만드는데 사용되는 생성자
	 * @param id
	 * @param pwd
	 * @param name
	 * @param lang
	 */
	public UserData(String id,String pwd,String name,byte lang) {
		this.id=id;
		this.pwd=pwd;
		this.name=name;
		this.lang=lang;
	}
	/**
	 * 사용자 데이터를 만드는데 사용되는 생성자
	 * @param name
	 * @param lang
	 * @param code
	 */
	public UserData(String name,byte lang,int code) {
		this.name=name;
		this.lang=lang;
		this.code=code;
	}
	
	public String getId() {
		return id;
	}
	public void setId(String id) {
		this.id = id;
	}
	public String getPwd() {
		return pwd;
	}
	public void setPwd(String pwd) {
		this.pwd = pwd;
	}
	public String getName() {
		return name;
	}
	public void setName(String name) {
		this.name = name;
	}
	public byte getLang() {
		return lang;
	}
	public void setLang(byte lang) {
		this.lang = lang;
	}
	public int getCode() {
		return code;
	}
	public void setCode(int code) {
		this.code = code;
	}
}
