#ifndef NTP_H
#define NTP_H

#define SNTP_SYNC_TIMEOUT_SEC   10
#define SNTP_SERVER_NAME      "pool.ntp.org"

void initialize_sntp(void);
volatile bool wait_for_sntp_sync();
void get_time_from_sntp(void *pvParameters);

#endif // NTP_H