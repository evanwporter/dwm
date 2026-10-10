{
	description = "dwm development workspace";

	inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";

	outputs = {
		self,
		nixpkgs,
	}: let
		system = "x86_64-linux";
		pkgs = nixpkgs.legacyPackages.${system};
		packages = import ./nix pkgs;
	in {
		packages.${system} = {
			default = packages.dwm;
			dwm = packages.dwm;
			dwm-tests = packages.dwm-tests;
		};

		app.${system} = {
			dwm = {
				type = "app";
				program = packages.dwm.out;
			};

			dwm-tests = {
				type = "app";
				program = packages.dwm-tests.out;
			};
		};

		hydraJobs = {
			dwm = packages.dwm;
			dwm-tests = packages.dwm-tests;
		};

		devShells.${system} = {
			default =
				pkgs.mkShell {
					inputsFrom = [self.packages.${system}.default];
					packages = with pkgs; [
						bear
						clang-tools
						criterion
						gnumake
						jq
						pkg-config
						kati
						ninja
					];
				};
		};
	};
}
