<?hh // strict
namespace Decouple\Repo\Console\Command;
use \Decouple\Console\Service\Log;
use \Decouple\Console\Service\Env;
class Install extends \Decouple\Console\Command {
  public function execute(Log $log, Env $env) : void {
    $log->log("Checking dependencies...");
    $config = \Decouple\Repo\Console\Service\Validator::getConfig($env);
    $validator = \Decouple\Repo\Console\Service\Validator::validate($config);
    $valid = (bool)$validator->get('valid');
    $installed = (int)$validator->get('gh_installed');
    $not_installed = (int)$validator->get('gh_not_installed');
    $outdated = (int)$validator->get('gh_outdated');
    $root = (string)$env->variables->get('root');
    if($valid) {
      if($outdated > 0) {
        exit("Outdated packages detected. Run `./decouple update` instead.\n");
      } else if($not_installed == 0) {
        exit("\nAll packages are installed and up to date\n");
      }
      // Install packages!!!
      $to_install = $validator->get('to_install');
      $full_command = '';
      @mkdir($root . '/vendor');
      chdir($root . '/vendor');
      if(is_array($to_install)) {
        foreach($to_install as $repo) {
          if(is_array($repo)) {
            list($repo,$version) = $repo;
            $repo = (string)$repo;
            $version = (string)$version;
            $command = sprintf("git submodule add -b %s https://github.com/%s.git ./vendor/%s", $version, $repo, $repo);
            $log->log(sprintf("Cloning %s:%s", $repo, $version));
            $output[] = getExecOutput($command, $root);
            $full_command .= $command . ' & ';
          }
        }
        $output[] = getExecOutput('git submodule update', $root);
        if($output) {
          $log->log("Installation complete.\n");
        }
      } else {
        exit("An unexpected error has occurred. Check the syntax in decouple.ini\n");
      }
    } else {
      exit("This project has not been initialized. Run `./decouple init` to begin.\n");
    }
  }
}
