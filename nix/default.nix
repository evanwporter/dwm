pkgs: {
	dwm =
		pkgs.stdenv.mkDerivation {
			pname = "dwm";
			version = "6.8";
			src = ../.;

			nativeBuildInputs = [pkgs.pkg-config];
			buildInputs = with pkgs; [
				fontconfig
				freetype
				libx11
				libXcursor
				libxcb
				libxft
				libxinerama
			];

			installFlags = ["PREFIX=$(out)"];
		};

	dwm-tests =
		pkgs.stdenv.mkDerivation {
			pname = "dwm-tests";
			version = "6.8";
			src = ../.;

			# installFlags = ["PREFIX=$(out)"];

			postPatch = ''
				ln -s config.def.h config.h
			'';

			buildPhase = ''
				runHook preBuild
				make tests
				runHook postBuild
			'';

			checkPhase = ''
				runHook preCheck
				./tests
				runHook postCheck
			'';

			doCheck = true;

			installPhase = ''
				runHook preInstall
				mkdir -p $out
				            # if we reached the install phase then the tests passed
				            # so we just put a random file in the output
				touch $out/tests-passes
				runHook postInstall
			'';

			buildInputs = with pkgs; [
				fontconfig
				freetype
				libX11
				libXcursor
				libxcb
				libXft
				libXinerama
				criterion
			];

			nativeBuildInputs = with pkgs; [
				pkg-config
			];
		};
}
