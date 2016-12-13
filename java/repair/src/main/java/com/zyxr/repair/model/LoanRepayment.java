package com.zyxr.repair.model;

import java.util.Date;

public class LoanRepayment {
	public long id;
	public String loansid;
	public int status;
	public int period;
	public int repaymode;
	public String stillPrincipal;
	public String stillInterest;
	public Date repaydate;
	public Date getRepaydate() {
		return repaydate;
	}
	public void setRepaydate(Date repaydate) {
		this.repaydate = repaydate;
	}
	public long getId() {
		return id;
	}
	public void setId(long id) {
		this.id = id;
	}
	public String getLoansid() {
		return loansid;
	}
	public void setLoansid(String loansid) {
		this.loansid = loansid;
	}
	public int getStatus() {
		return status;
	}
	public void setStatus(int status) {
		this.status = status;
	}
	public int getPeriod() {
		return period;
	}
	public void setPeriod(int period) {
		this.period = period;
	}
	public int getRepaymode() {
		return repaymode;
	}
	public void setRepaymode(int repaymode) {
		this.repaymode = repaymode;
	}
	public String getStillPrincipal() {
		return stillPrincipal;
	}
	public void setStillPrincipal(String stillPrincipal) {
		this.stillPrincipal = stillPrincipal;
	}
	public String getStillInterest() {
		return stillInterest;
	}
	public void setStillInterest(String stillInterest) {
		this.stillInterest = stillInterest;
	}
	@Override
	public String toString() {
		return "LoanRepayment [id=" + id + ", loansid=" + loansid + ", status=" + status + ", period=" + period
				+ ", repaymode=" + repaymode + ", stillPrincipal=" + stillPrincipal + ", stillInterest=" + stillInterest
				+ ", repaydate=" + repaydate + "]";
	}
	
	public boolean isRepaymented() {
		if (3==status) {
			return true;
		}
		return false;
	}
}
