#!/bin/sh
# does not work now
curl -X PUT -L -k --data "ClientTransactionID=24&ClientID=31927" http://127.0.0.1:11111/management/v1/description

# curl --data "ClientTransactionID=24&ClientID=31927&Connected=True" http://127.0.0.1:11111/api/v1/telescope/0/connected