# harmonia

Harmonia is a binary cache for nix that serves your /nix/store as a binary cache over http.
It's written in Rust for speed.

## Features

- http-ranges support for nar file streaming
- streaming build logs
- .ls file streaming
  - Note: doesn't contain `narOffset` in json response but isn't needed for
    `nix-index`
- Add `/serve/<narhash>/` endpoint to allow serving the content of package. 
  Also discovers index.html to allow serving websites directly from the nix store.

## Configuration for public binary cache on NixOS

There is a module for harmonia in nixpkgs.
The following example set's up harmonia as a public binary cache using
nginx as a frontend webserver with https encryption and zstd for compression:

```nix
{ config, pkgs, ... }: {
  services.harmonia.enable = true;
  # FIXME: generate a public/private key pair like this:
  # $ nix-store --generate-binary-cache-key cache.yourdomain.tld-1 /var/lib/secrets/harmonia.secret /var/lib/secrets/harmonia.pub
  services.harmonia.signKeyPaths = [ "/var/lib/secrets/harmonia.secret" ];
  # Example using sops-nix to store the signing key
  #services.harmonia.signKeyPaths = [ config.sops.secrets.harmonia-key.path ];
  #sops.secrets.harmonia-key = { };

  # optional if you use allowed-users in other places
  #nix.settings.allowed-users = [ "harmonia" ];

  networking.firewall.allowedTCPPorts = [ 443 80 ];

  # FIXME: replace this with your own email
  security.acme.defaults.email = "yourname@youremail.com";
  security.acme.acceptTerms = true;

  services.nginx = {
    enable = true;
    recommendedTlsSettings = true;
    # FIXME: replace "cache.yourdomain.tld" with your own domain.
    virtualHosts."cache.yourdomain.tld" = {
      enableACME = true;
      forceSSL = true;

      locations."/".extraConfig = ''
        proxy_pass http://127.0.0.1:5000;
        proxy_set_header Host $host;
        proxy_redirect http:// https://;
        proxy_http_version 1.1;
        proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
        proxy_set_header Upgrade $http_upgrade;
        proxy_set_header Connection $connection_upgrade;
      '';
    };
  };
}
```

You can use the binary cache on a different machine using the following NixOS configuration:

```nix
{
  nix.settings = {
    substituters = [ "https://cache.yourdomain.tld" ];
    # FIXME replace the key with the content of /var/lib/secrets/harmonia.pub
    trusted-public-keys = [ "cache.yourdomain.tld-1:AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA=" ];
  };
}
```

## Configuration format

Configuration is done via a `toml` file.
**Hint:** You don't need to interface with the configuration directly in case you are using the NixOS module.
The location of the configuration file should be passed as env var `CONFIG_FILE`. If no config file is passed the
following default values will be used:

```toml
# default ip:hostname to bind to
bind = "[::]:5000"
# unix socket are also supported
# bind = "unix:/run/harmonia/socket"
# Sets number of workers to start in the webserver
workers = 4
# Sets the per-worker maximum number of concurrent connections.
max_connection_rate = 256
# binary cache priority that is advertised in /nix-cache-info
priority = 30

# Allow to override the store path advertised in /nix-cache-info
# virtual_nix_store = "/nix/store"
# Allow to serve the nix store from a different physical location
# Default: empty
# Example: if you use `nix copy --store /guest` to populate a store than configure:
# real_nix_store = "/guest/nix/store"
```

Per default we wont sign any narinfo because we don't have a secret key, to
enable this feature enable it by providing a path to a private key generated by
`nix-store --generate-binary-cache-key cache.example.com-1 /etc/nix/cache.secret /etc/nix/cache.pub`

```toml
# nix binary cache signing key
sign_key_paths = [ "/run/secrets/cache.secret" ]
```

Harmonia also reads the `SIGN_KEY_PATHS` environment variable which holds paths to secret keys separated by spaces.
All paths provided by `sign_key_paths` config option and `SIGN_KEY_PATHS` environment variable will be used for signing.

Logging can be configured with
[env_logger](https://docs.rs/env_logger/latest/env_logger/). The default value
is `info,actix_web=debug`. To only log errors use the following
`RUST_LOG=error` and to only disable access logging, use
`RUST_LOG=info,actix_web::middleware=error`

To enable TLS on the HTTP server, specify `tls_cert_path` and `tls_key_path`.

## Build

### Whole application

```bash
nix build -L
```

### Get a development environment:

``` bash
nix develop
```

## Run tests

```bash
nix flake check -L
```


## Inspiration

- [eris](https://github.com/thoughtpolice/eris)
