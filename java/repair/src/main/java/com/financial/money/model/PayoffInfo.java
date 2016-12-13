package com.financial.money.model;

import java.util.Date;

public class PayoffInfo {
	public long payoff_id;//       			bigint not null auto_increment comment '投资收益单ID',
	public long asset_id;//            		bigint not null default 0 comment '标的ID',	 
	public String asset_name;// 					varchar(64) not null default '' comment '标名',
	public int asset_type;//					tinyint not null default 0 comment '标的类型,0-散标,1-理财计划',
	public int asset_pool;//					tinyint not null default 0 comment '理财计划池/散标池,0-表示无,1-表示散标池,2-表示理财计划池,3-散标池和理财计划池',
	public long investor_uid;//     			bigint not null default 0 comment '真实投资人UID',
	public long borrower_uid;//       			bigint not null default 0 comment '借款人UID',
	public long repayment_id;//     			bigint not null default 0 comment '还款计划ID',
	public int phase;//          				int not null default 0 comment '第几期收益',		
	public long actual_principal;//			bigint not null default 0 comment '投资人实际已收到的本金',
	public long actual_interest;//  			bigint not null default 0 comment '投资人实际已收到的利息',
	public long actual_add_interest;//			bigint not null default 0 comment '平台实际给投资人的加息,单位:分',
	public long actual_pay_platform;//			bigint not null default 0 comment '投资人交钱给平台',
//	public Date actual_date;//  				datetime not null default '0000-00-00 00:00:00' comment '投资人本期实际获取本金收益,获取收益后给平台费率的时间',
	public int state;//             			tinyint not null default 0 comment '状态：1-待还，2-本期收益完毕',
	public long property;// 				bigint not null default 0 comment '0-正常,1-收益转入中间账户',
	
	public long getPayoff_id() {
		return payoff_id;
	}
	public void setPayoff_id(long payoff_id) {
		this.payoff_id = payoff_id;
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
	public int getAsset_type() {
		return asset_type;
	}
	public void setAsset_type(int asset_type) {
		this.asset_type = asset_type;
	}
	public int getAsset_pool() {
		return asset_pool;
	}
	public void setAsset_pool(int asset_pool) {
		this.asset_pool = asset_pool;
	}
	public long getInvestor_uid() {
		return investor_uid;
	}
	public void setInvestor_uid(long investor_uid) {
		this.investor_uid = investor_uid;
	}
	public long getBorrower_uid() {
		return borrower_uid;
	}
	public void setBorrower_uid(long borrower_uid) {
		this.borrower_uid = borrower_uid;
	}
	public long getRepayment_id() {
		return repayment_id;
	}
	public void setRepayment_id(long repayment_id) {
		this.repayment_id = repayment_id;
	}
	public int getPhase() {
		return phase;
	}
	public void setPhase(int phase) {
		this.phase = phase;
	}
	public long getActual_principal() {
		return actual_principal;
	}
	public void setActual_principal(long actual_principal) {
		this.actual_principal = actual_principal;
	}
	public long getActual_interest() {
		return actual_interest;
	}
	public void setActual_interest(long actual_interest) {
		this.actual_interest = actual_interest;
	}
	public long getActual_add_interest() {
		return actual_add_interest;
	}
	public void setActual_add_interest(long actual_add_interest) {
		this.actual_add_interest = actual_add_interest;
	}
	public long getActual_pay_platform() {
		return actual_pay_platform;
	}
	public void setActual_pay_platform(long actual_pay_platform) {
		this.actual_pay_platform = actual_pay_platform;
	}
//	public Date getActual_date() {
//		return actual_date;
//	}
//	public void setActual_date(Date actual_date) {
//		this.actual_date = actual_date;
//	}
	public int getState() {
		return state;
	}
	public void setState(int state) {
		this.state = state;
	}
	public long getProperty() {
		return property;
	}
	public void setProperty(long property) {
		this.property = property;
	}
	@Override
	public String toString() {
		return "PayoffInfo [payoff_id=" + payoff_id + ", asset_id=" + asset_id + ", asset_name=" + asset_name
				+ ", asset_type=" + asset_type + ", asset_pool=" + asset_pool + ", investor_uid=" + investor_uid
				+ ", borrower_uid=" + borrower_uid + ", repayment_id=" + repayment_id + ", phase=" + phase
				+ ", actual_principal=" + actual_principal + ", actual_interest=" + actual_interest
				+ ", actual_add_interest=" + actual_add_interest + ", actual_pay_platform=" + actual_pay_platform
				+ ", state=" + state + ", property=" + property + "]\n";
	}
	
	public String title() {
		return "标id,标题,还款id,回款id,回款期号,投资人uid,姓名,手机号,本金,利息,本息\n";
	}
	
	public String getOut() {
		return String.format("%d,%s,%d,%d,%d,%d,%s,%s,%d,%d,%d\n", 
				asset_id,
				asset_name,
				repayment_id,
				payoff_id,
				phase,
				investor_uid,
				"",
				"",
				actual_principal,
				actual_interest,
				actual_principal+actual_interest);
	}
	
}
