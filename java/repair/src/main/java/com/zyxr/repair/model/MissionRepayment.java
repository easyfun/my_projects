package com.zyxr.repair.model;

import java.util.Date;

public class MissionRepayment {
	public long id;
	public String loan_id;
	public int status;
	public int period;
	public int repay_mode;
	public String still_principal;
	public String still_interest;
	public Date repay_date;
	public Date getRepay_date() {
		return repay_date;
	}

	public void setRepay_date(Date repay_date) {
		this.repay_date = repay_date;
	}


	//无库
	public String borrower_id;

	public long getId() {
		return id;
	}

	public void setId(long id) {
		this.id = id;
	}

	public String getLoan_id() {
		return loan_id;
	}

	public void setLoan_id(String loan_id) {
		this.loan_id = loan_id;
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

	public int getRepay_mode() {
		return repay_mode;
	}

	public void setRepay_mode(int repay_mode) {
		this.repay_mode = repay_mode;
	}

	public String getStill_principal() {
		return still_principal;
	}

	public void setStill_principal(String still_principal) {
		this.still_principal = still_principal;
	}

	public String getStill_interest() {
		return still_interest;
	}

	public void setStill_interest(String still_interest) {
		this.still_interest = still_interest;
	}

	public String getBorrower_id() {
		return borrower_id;
	}

	public void setBorrower_id(String borrower_id) {
		this.borrower_id = borrower_id;
	}

	@Override
	public String toString() {
		return "MissionRepayment [id=" + id + ", loan_id=" + loan_id + ", status=" + status + ", period=" + period
				+ ", repay_mode=" + repay_mode + ", still_principal=" + still_principal + ", still_interest="
				+ still_interest + ", repay_date=" + repay_date + ", borrower_id=" + borrower_id + "]";
	}
	
	
	public Boolean isRepaymented() {
		if (3==status) {
			return true;
		}
		return false;
	}
}
