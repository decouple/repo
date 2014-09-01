<?hh // strict
namespace Decouple\Repo\Console\Service;
use \Decouple\Console\Service\Env;
class Validator {
  public static function getConfig(Env $env) : Map<string,mixed> {
    /** Get the application root directory **/
    $dir = (string)$env->variables->get('root');
    /** Check for a decouple.ini in application root **/
    $file = $dir . '/decouple.ini';
    if (!file_exists($file)) {
      throw new \Exception("This project has not been initialized. Run `./decouple init` to begin.");
    }
    $config = new Map(parse_ini_file($file,true));
    $config->set('root', (string)$env->variables->get('root'));
    return $config;
  }
  public static function validate(Map<string,mixed> $config) : Map<string,mixed> {
    /** Validation defaults **/
    $valid = true;
    $gh_not_installed = 0;
    $gh_installed = 0;
    $gh_outdated = 0;
    $dir = (string)$config->get('root');
    $gh_to_install = [];
    /** Check dependencies **/
    if($config->containsKey('dependencies')) {
      $dependencies = $config->get('dependencies');
      if(is_array($dependencies)) {
          $dependencies = new Map($dependencies);
        } else {
          $dependencies = Map {};
        }
      /** Check for Github dependencies **/
      if($dependencies->containsKey('github')) {
        $github = $dependencies->get('github');
        if(is_array($github) || $github instanceof KeyedTraversable) {
          $github = new Map($github);
        } else {
          $github = Map {};
        }
        /** Iterate github lines **/
        foreach($github as $line) {
          list($repo, $version) = explode(':', $line);
          /** Make sure we have a repo **/
          if(is_null($repo)) {
            throw new \Exception(sprintf("Invalid repository requested: %s", $line));
          }
          /** If no version supplied, default to master **/
          if(is_null($version)) {
            $version = "master";
          }
          /** Cast to string **/
          $repo = (string)$repo;
          $version = (string)$version;
          /** Prepare github URL and local reo directory **/
          $repo_check_url = 'https://github.com/' . $repo . '.git';
          $repo_dir = $dir . '/vendor/' . $repo;
          /** If the directory doesn't exist, it's not installed **/
          if(!is_dir($repo_dir)) {
            $gh_not_installed++;
            $gh_to_install[] = [$repo,$version];
          } else {
            /** If the directory exists, make sure we have a repository and it's up to date **/
            $gh_installed++;
            $status = getExecOutput('git fetch --dry-run', $repo_dir);
            /** Remove script start & stop lines **/
            array_shift($status); array_pop($status);
            if(count($status)) {
              var_dump($status);
              $gh_outdated++;
            }
          }
        }
      }
    }
    return Map {
      "valid" => (int)$valid,
      "gh_installed" => $gh_installed,
      "gh_outdated" => $gh_outdated,
      "gh_not_installed" => $gh_not_installed,
      "to_install" => $gh_to_install
    };
  }
}
