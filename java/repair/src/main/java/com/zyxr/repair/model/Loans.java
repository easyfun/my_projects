package com.zyxr.repair.model;

public class Loans {
	public String id;
	public String title;
	public int status;
	public String borrower;
	public String borrowername;
	public String borrowamount;
	public String annualrate;
	public String add_rate;
	public int periods;
	public int periodmode;
	public int repaymode;
	public String getId() {
		return id;
	}
	public void setId(String id) {
		this.id = id;
	}
	public String getTitle() {
		return title;
	}
	public void setTitle(String title) {
		this.title = title;
	}
	public int getStatus() {
		return status;
	}
	public void setStatus(int status) {
		this.status = status;
	}
	public String getBorrower() {
		return borrower;
	}
	public void setBorrower(String borrower) {
		this.borrower = borrower;
	}
	public String getBorrowername() {
		return borrowername;
	}
	public void setBorrowername(String borrowername) {
		this.borrowername = borrowername;
	}
	public String getBorrowamount() {
		return borrowamount;
	}
	public void setBorrowamount(String borrowamount) {
		this.borrowamount = borrowamount;
	}
	public String getAnnualrate() {
		return annualrate;
	}
	public void setAnnualrate(String annualrate) {
		this.annualrate = annualrate;
	}
	public String getAdd_rate() {
		return add_rate;
	}
	public void setAdd_rate(String add_rate) {
		this.add_rate = add_rate;
	}
	public int getPeriods() {
		return periods;
	}
	public void setPeriods(int periods) {
		this.periods = periods;
	}
	public int getPeriodmode() {
		return periodmode;
	}
	public void setPeriodmode(int periodmode) {
		this.periodmode = periodmode;
	}
	public int getRepaymode() {
		return repaymode;
	}
	public void setRepaymode(int repaymode) {
		this.repaymode = repaymode;
	}
	@Override
	public String toString() {
		return "Loans [id=" + id + ", title=" + title + ", status=" + status + ", borrower=" + borrower
				+ ", borrowername=" + borrowername + ", borrowamount=" + borrowamount + ", annualrate=" + annualrate
				+ ", add_rate=" + add_rate + ", periods=" + periods + ", periodmode=" + periodmode + ", repaymode="
				+ repaymode + "]\n";
	}
	
	
	
	
}
