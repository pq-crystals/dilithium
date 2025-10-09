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
        dilithium-lib = pkgs.stdenv.mkDerivation {
            name = "dilithium-lib";
            src = pkgs.lib.cleanSourceWith {
              src = ./.;
              filter = path: type:
                let baseName = baseNameOf path;
                in !(baseName == "example" ||
                     baseName == "flake.nix" ||
                     baseName == "flake.lock");
            };

            nativeBuildInputs = with pkgs; [
              clang
              openssl
            ];

            buildPhase = ''
              # Save the current directory to buildDir
              buildDir=$PWD

              # Build reference implementation with shared libraries
              cd ref
              make -j$NIX_BUILD_CORES
              make shared
              
              # Build all tests for reference implementation
              for alg in 2 3 5; do
                make test/test_dilithium$alg
                make test/test_vectors$alg
              done
              
              # Build AVX2 implementation if supported
              if [ "$(uname -m)" = "x86_64" ]; then
                cd ../avx2
                make -j$NIX_BUILD_CORES
                make shared
                
                # Build all tests for AVX2 implementation
                for alg in 2 3 5; do
                  make test/test_dilithium$alg
                  make test/test_vectors$alg
                done
              fi
            '';

            installPhase = ''
              mkdir -p $out/lib $out/include/dilithium
              
              cp -r $src/ref/*.h $out/include/dilithium/
              cp -r $buildDir/ref/libpqcrystals_*_ref.so $out/lib/
              
              # Install AVX2 implementation libraries if built
              if [ "$(uname -m)" = "x86_64" ]; then
                mkdir $out/include/dilithium/avx2
                cp -r $src/avx2/*.h $out/include/dilithium/avx2/
                cp -r $buildDir/avx2/libpqcrystals_*_avx2.so $out/lib/
              fi
            '';

            checkPhase = ''
              mkdir -p $TMPDIR/test-outputs
              
              # Run tests
              for alg in 2 3 5; do
                ./test/test_dilithium$alg
                ./test/test_vectors$alg > $TMPDIR/test-outputs/tvecs$alg
              done

              # Verify checksums from temporary directory
              cd $TMPDIR/test-outputs
              sha256sum -c $src/SHA256SUMS
            '';

            doCheck = true;
          };
          dilithium = pkgs.stdenv.mkDerivation {
            name = "dilithium";
            src = ./.;
            propagatedBuildInputs = [ dilithium-lib ];

            nativeBuildInputs = with pkgs; [
              gcc
              openssl
            ];
            checkInputs = with pkgs; [
              bash
            ];

            buildPhase = ''
              export DILITHIUM_INCLUDE_DIR="${dilithium-lib}/include"
              export DILITHIUM_LIB_DIR="${dilithium-lib}/lib"
              export LD_LIBRARY_PATH="${dilithium-lib}/lib:$LD_LIBRARY_PATH"

              cd example
              if [ "$(uname -m)" = "x86_64" ]; then
                make -j$NIX_BUILD_CORES USE_AVX2=1
              else
                make -j$NIX_BUILD_CORES
              fi
            '';

            installPhase = ''
              mkdir -p $out/bin $out/lib
              
              # Install example binaries for all Dilithium variants
              for variant in 2 3 5; do
                cp dilithium$variant-keygen $out/bin/
                cp dilithium$variant-sign $out/bin/
                cp dilithium$variant-verify $out/bin/
              done

              # Install the main wrapper script
              cp $src/example/dilithium $out/bin/
              chmod +x $out/bin/dilithium

              # Set up library path in the wrapper script
              substituteInPlace $out/bin/dilithium \
                --replace '#!/usr/bin/env bash' '#!${pkgs.bash}/bin/bash' \
                --replace 'BINARY_DIR="$(dirname "$0")"' "BINARY_DIR=$out/bin" \
                --replace 'export LD_LIBRARY_PATH=LD_LIBRARY_PATH' 'export LD_LIBRARY_PATH="${dilithium-lib}/lib:$LD_LIBRARY_PATH"'
            '';
          };
      in
      {
        packages = {
          inherit dilithium;
          default = dilithium;
          lib = dilithium-lib;
        };

        devShells.default = pkgs.mkShell {
          buildInputs = with pkgs; [
            clang
            openssl
            dilithium-lib
            dilithium
          ];
          
          shellHook = ''
            export CFLAGS="-I${pkgs.openssl.dev}/include -I${dilithium-lib}/include"
            export NISTFLAGS="-I${pkgs.openssl.dev}/include"
            export LDFLAGS="-L${pkgs.openssl.out}/lib -L${dilithium-lib}/lib"
          '';
        };
      }
    );
} 