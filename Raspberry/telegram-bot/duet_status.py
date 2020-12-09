import time
import requests

class DuetStatus:
    def __init__(self, duet_ip:str, expiration_time:float=1.0):
        self._json_status = None
        self._last_update_time = time.time()

        self._duet_ip = duet_ip
        self._send_request_status = "http://{}/rr_status?type=1".format(duet_ip)

        self._expiration_time = expiration_time

    def set_duet_ip(self, duet_ip):
        self._duet_ip = duet_ip
        self._send_request_status = "http://{}/rr_status?type=1".format(duet_ip)

    def set_expiration_time(self, expiration_time:float):
        self._expiration_time = expiration_time

    def get_homed(self, wait=False):
        self._update_json_status(wait)
        try:
            return self._json_status['coords']['axesHomed']
        except Exception as exc:
            self._log_exc(exc)
            return [0, 0, 0]

    def get_new_xyz(self, wait=False):
        self._update_json_status(wait)
        try:
            return self._json_status['coords']['xyz']
        except Exception as exc:
            self._log_exc(exc)
            return [-999.9, -999.9, -999.9]

    def get_old_xyz(self, wait=False):
        self._update_json_status(wait)
        try:
            return self._json_status['coords']['machine']
        except Exception as exc:
            self._log_exc(exc)
            return self.get_new_xyz()

    def _update_json_status(self, wait=False):
        if self._is_update_needed(wait):
            try:
                self._last_update_time = time.time()
                self._json_status = requests.get(self._send_request_status).json()
            except Exception as exc:
                self._log_exc(exc)

    def _is_update_needed(self, wait=False):
        if self._json_status is None:
            return True
        t = time.time()
        if wait:
            sleep_time = (self._last_update_time+self._expiration_time) - t
            if sleep_time > 0:
                time.sleep(sleep_time)
            return True
        return t-self._last_update_time > self._expiration_time

    @staticmethod
    def _log_exc(exc:Exception):
        print('DuetStatus:', str(exc))
