# server

Run a simple webservice that listen on port 8537.

## Compile and run

```bash
$ make test
```

### https connection

If you want this program to listen to a secure (https) connection, create a certificate with its certificate and private keys in to separate files, and run the program with the options `-secure <key_file> <cert_file>`.

## Endpoints available:

- `GET http://localhost:8537/podstate`: A "pod status" response
- `POST http://localhost:8537/podstate`: create the container
