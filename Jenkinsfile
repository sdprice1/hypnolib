#!groovy

node {
    wrap([$class: 'TimestamperBuildWrapper']) {

    def jobname = env.JOB_NAME
    def buildnum = env.BUILD_NUMBER.toInteger()
	def builddir = "/data/proj"
	def workspace = "${builddir}/${jobname}"

	ws("${workspace}") {

		// Mark the code checkout 'stage'....
		stage 'Checkout'

        def repo = 'hypnocube'
		doCheckout(repo, "master")


        dir(repo) {
            
			// Run the build
			stage 'Build'
	        sh "make cppcheck"
	        sh "make clean"
	        sh "make"
	
			// Run tests
			stage 'Test'
	        sh "make cppcheck"
	        sh "make test"
	
			// Mark the code build 'stage'....
			stage 'Post-build'
	//        step([$class: 'ArtifactArchiver', artifacts: "applications/${customer}-${app}/build/ppc-${board}-${customer}-${app}/packages/*", fingerprint: true])
	        step([$class: 'JUnitResultArchiver', testResults: '**/gtest.xml'])
	
	        }
		}

    }
}

def doCheckout(repo, branch) {

	checkout([
		$class: 'GitSCM',
		branches: [[name: "*/$branch"]],
		doGenerateSubmoduleConfigurations: false,
		extensions: [
			[
				$class: 'CheckoutOption',
				timeout: 20
			],
			[
				$class: 'RelativeTargetDirectory',
				relativeTargetDir: "$repo",
			],
			[
				$class: 'CloneOption',
				noTags: false,
				reference: '',
				shallow: true
			]
		],
		submoduleCfg: [],
		userRemoteConfigs: [[url: "ssh://git@quartz/var/lib/git/${repo}.git"]]
	])
}

