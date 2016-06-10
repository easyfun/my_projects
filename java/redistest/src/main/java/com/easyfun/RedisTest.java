package com.easyfun;

import redis.clients.jedis.Jedis;

public class RedisTest {
	public static void main(String[] args)  {
		final int THREADNUM=100;
		RedisWorker[] workers=new RedisWorker[THREADNUM];
		for (int i=0;i<THREADNUM;i++)	{
			workers[i]=new RedisWorker();
			Thread req_thread=new Thread(workers[i]);
			req_thread.start();
		}
		
		while (true) {
			try {
				Thread.sleep(500);
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
			double speed=0.0;
			for (int i=0;i<THREADNUM;i++){
				speed+=workers[i].getStat();
			}
			System.out.println("speed="+speed+" qps");
		}
	}
}

class RedisWorker implements Runnable {
	double counter;
	long startTime;

	RedisWorker() {
		counter=0.0;
		startTime=System.currentTimeMillis();
	}
	
	public void run() {
		startTime=System.currentTimeMillis();
		@SuppressWarnings("resource")
		Jedis client=new Jedis("192.168.1.101", 6379);
		client.connect();
		int i=0;
		while(true) {
			client.set("foo"+i, "bar"+i);
			client.get("foo"+i);
			i++;
			if (i >= 50000) {
				i=0;
			}
			counter++;
		}
		//client.disconnect();
	}
	
	double getStat(){
		return counter*1000/(System.currentTimeMillis()-startTime);
	}
} 