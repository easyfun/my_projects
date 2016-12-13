package com.zyxr.repair.model;

import java.util.Date;

public class AssetInfo {
	public long  asset_id;
	public String  asset_name;
	public long  borrower_uid;
	public String  borrower_name;
	public long  total_amount;
	public long  annual_rate;
	public long  add_rate;
	public int  phase_total;
	public int  phase_mode;
	public int  repay_mode;
	public Date full_time;
	
	public Date getFull_time() {
		return full_time;
	}
	public void setFull_time(Date full_time) {
		this.full_time = full_time;
	}
	public long getAsset_id() {
		return asset_id;
	}
	public void setAsset_id(long asset_id) {
		this.asset_id = asset_id;
	}
	public String getAsset_name() {
		return asset_name;
	}
	public void setAsset_name(String asset_name) {
		this.asset_name = asset_name;
	}
	public long getBorrower_uid() {
		return borrower_uid;
	}
	public void setBorrower_uid(long borrower_uid) {
		this.borrower_uid = borrower_uid;
	}
	public String getBorrower_name() {
		return borrower_name;
	}
	public void setBorrower_name(String borrower_name) {
		this.borrower_name = borrower_name;
	}
	public long getTotal_amount() {
		return total_amount;
	}
	public void setTotal_amount(long total_amount) {
		this.total_amount = total_amount;
	}
	public long getAnnual_rate() {
		return annual_rate;
	}
	public void setAnnual_rate(long annual_rate) {
		this.annual_rate = annual_rate;
	}
	public long getAdd_rate() {
		return add_rate;
	}
	public void setAdd_rate(long add_rate) {
		this.add_rate = add_rate;
	}
	public int getPhase_total() {
		return phase_total;
	}
	public void setPhase_total(int phase_total) {
		this.phase_total = phase_total;
	}
	public int getPhase_mode() {
		return phase_mode;
	}
	public void setPhase_mode(int phase_mode) {
		this.phase_mode = phase_mode;
	}
	public int getRepay_mode() {
		return repay_mode;
	}
	public void setRepay_mode(int repay_mode) {
		this.repay_mode = repay_mode;
	}
	@Override
	public String toString() {
		return "AssetInfo [asset_id=" + asset_id + ", asset_name=" + asset_name + ", borrower_uid=" + borrower_uid
				+ ", borrower_name=" + borrower_name + ", total_amount=" + total_amount + ", annual_rate=" + annual_rate
				+ ", add_rate=" + add_rate + ", phase_total=" + phase_total + ", phase_mode=" + phase_mode
				+ ", repay_mode=" + repay_mode + ", full_time=" + full_time + "]";
	}
	
	
}
