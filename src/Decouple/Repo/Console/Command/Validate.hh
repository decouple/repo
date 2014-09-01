<?hh // strict
namespace Decouple\Repo\Console\Command;
use \Decouple\Console\Service\Log;
use \Decouple\Console\Service\Env;
class Validate extends \Decouple\Console\Command {
  public function execute(Log $log, Env $env) : void {
    $log->log("Validating configuration...");
    $lambda = (...) ==> print_r(func_get_args()[0]);
    $config = \Decouple\Repo\Console\Service\Validator::getConfig($env);
    $validator = \Decouple\Repo\Console\Service\Validator::validate($config);
    $gh_not_installed = (int)$validator->get('gh_not_installed');
    $gh_installed = (int)$validator->get('gh_installed');
    $gh_outdated = (int)$validator->get('gh_outdated');
    $valid = (int)$validator->get('valid');
    if($gh_not_installed > 0) {
      if($gh_installed > 0) {
        exit(sprintf("\nInstallation invalid. %s packages must be installed. Run `./decouple update`\n", $gh_not_installed));
      } else {
        exit(sprintf("\nInstallation invalid. No packages are installed, and %s are pending installation. Run `./decouple install`\n", $gh_not_installed));
      }
    } else {
      exit("\nThis is a valid Decouple installation.\n");
    }
  }
}
