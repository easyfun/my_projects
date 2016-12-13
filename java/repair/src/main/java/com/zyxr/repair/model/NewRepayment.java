package com.zyxr.repair.model;

import java.util.Date;

public class NewRepayment {
	public int state;

	public int getState() {
		return state;
	}

	public void setState(int state) {
		this.state = state;
	}

	@Override
	public String toString() {
		return "NewRepayment [state=" + state + "]";
	}
	
}



