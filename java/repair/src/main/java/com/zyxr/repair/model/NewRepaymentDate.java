package com.zyxr.repair.model;

import java.util.Date;

public class NewRepaymentDate {
	public int state;
	public Date expect_date;
	public int getState() {
		return state;
	}
	public void setState(int state) {
		this.state = state;
	}
	public Date getExpect_date() {
		return expect_date;
	}
	public void setExpect_date(Date expect_date) {
		this.expect_date = expect_date;
	}
	@Override
	public String toString() {
		return "NewRepaymentDate [state=" + state + ", expect_date=" + expect_date + "]";
	}


}