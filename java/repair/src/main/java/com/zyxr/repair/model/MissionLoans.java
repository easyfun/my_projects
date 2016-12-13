package com.zyxr.repair.model;

public class MissionLoans {
	public String id;
	public String title;
	public int status;
	public String borrower_id;
	public String borrower_name;
	public String getBorrower_name() {
		return borrower_name;
	}
	public void setBorrower_name(String borrower_name) {
		this.borrower_name = borrower_name;
	}
	public String amount;
	public String annual_rate;
	//public String add_annual_rate;
	public int periods;
	//public int period_type;
	public int repay_mode;
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
	public String getBorrower_id() {
		return borrower_id;
	}
	public void setBorrower_id(String borrower_id) {
		this.borrower_id = borrower_id;
	}
	public String getAmount() {
		return amount;
	}
	public void setAmount(String amount) {
		this.amount = amount;
	}
	public String getAnnual_rate() {
		return annual_rate;
	}
	public void setAnnual_rate(String annual_rate) {
		this.annual_rate = annual_rate;
	}
	public int getPeriods() {
		return periods;
	}
	public void setPeriods(int periods) {
		this.periods = periods;
	}
//	public int getPeriod_type() {
//		return period_type;
//	}
//	public void setPeriod_type(int period_type) {
//		this.period_type = period_type;
//	}
	public int getRepay_mode() {
		return repay_mode;
	}
	public void setRepay_mode(int repay_mode) {
		this.repay_mode = repay_mode;
	}
	@Override
	public String toString() {
		return "MissionLoans [id=" + id + ", title=" + title + ", status=" + status + ", borrower_id=" + borrower_id
				+ ", borrower_name=" + borrower_name + ", amount=" + amount + ", annual_rate=" + annual_rate
				+ ", periods=" + periods + ", repay_mode=" + repay_mode + "]\n";
	}
	
	
}
