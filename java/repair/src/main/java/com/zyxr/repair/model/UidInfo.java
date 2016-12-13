package com.zyxr.repair.model;

public class UidInfo {
	public long userid;
	public String ybuid;
	public String getYbuid() {
		return ybuid;
	}
	public long getUserid() {
		return userid;
	}
	public void setUserid(long userid) {
		this.userid = userid;
	}
	public void setYbuid(String ybuid) {
		this.ybuid = ybuid;
	}
	@Override
	public String toString() {
		return "UidInfo [userid=" + userid + ", ybuid=" + ybuid + "]\n";
	}
	
	
}
