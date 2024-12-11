{
  description = "Dilithium post-quantum cryptography implementation";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
      in
      {
        packages.default = pkgs.stdenv.mkDerivation {
          name = "dilithium";
          src = ./.;

          nativeBuildInputs = with pkgs; [
            clang
            openssl
          ];

          buildPhase = ''
            # Build reference implementation
            make -C ref -j$NIX_BUILD_CORES
            
            # Build AVX2 implementation if supported
            if [ "$(uname -m)" = "x86_64" ]; then
              make -C avx2 -j$NIX_BUILD_CORES
            fi
          '';

          installPhase = ''
            mkdir -p $out/bin
            
            # Install reference implementation binaries
            for alg in 2 3 5; do
              cp ref/test/test_dilithium$alg $out/bin/
              cp ref/test/test_vectors$alg $out/bin/
            done

            # Install AVX2 binaries if built
            if [ "$(uname -m)" = "x86_64" ]; then
              for alg in 2 3 5; do
                cp avx2/test/test_dilithium$alg $out/bin/test_dilithium$alg-avx2
                cp avx2/test/test_vectors$alg $out/bin/test_vectors$alg-avx2
              done
            fi
          '';

          checkPhase = ''
            # Run tests for reference implementation
            for alg in 2 3 5; do
              ref/test/test_dilithium$alg
              ref/test/test_vectors$alg > tvecs$alg
            done

            # Run tests for AVX2 implementation if available
            if [ "$(uname -m)" = "x86_64" ]; then
              for alg in 2 3 5; do
                avx2/test/test_dilithium$alg
                avx2/test/test_vectors$alg > tvecs$alg-avx2
              done
            fi

            sha256sum -c SHA256SUMS
          '';

          doCheck = true;
        };

        devShells.default = pkgs.mkShell {
          buildInputs = with pkgs; [
            clang
            openssl
          ];
        };
      }
    );
} 