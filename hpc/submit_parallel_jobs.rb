#!/usr/bin/env ruby
require 'optparse'
require 'fileutils'


def puts_lightblue(text)
  puts "\e[94m#{text}\e[0m"
end

def puts_lightred(text)
  puts "\e[91m#{text}\e[0m"
end

def puts_lightgreen(text)
  puts "\e[92m#{text}\e[0m"
end

def run_executable(exe)
  # Execute the command and print its output in real-time
  IO.popen(exe) do |output|
      while line = output.gets
        puts line
      end
  end
end


options = {
  run_card: './runcards/stringSpinSim.card',
  events: 1000,
  output_dir: './out/',
  num_batches: 1
}

# Check if the executable exists in the specified directory
def valid_executable?(file)
  File.exist?("./pythia_programs/#{file}")
end

# Check if a file or directory exists
def valid_path?(path)
  File.exist?(path)
end

OptionParser.new do |opts|
  opts.banner = "Usage: submit_parallel_jobs.rb [options]"

  opts.on("-n", "--name NAME", "Project name.") do |n|
    if n.nil? || n.strip.empty?
      puts "Error: Project name (-n) cannot be empty."
      puts opts
      exit
    else
      options[:project_name] = n
    end
  end

  opts.on("-e", "--executable EXECUTABLE", "Executable name.") do |e|
    executable_path = "./pythia_programs/#{e}"
    if valid_executable?(e)
      options[:executable_name] = executable_path
    else
      puts "Error: Executable '#{e}' does not exist in './pythia_programs'."
      exit
    end
  end

  opts.on("-r", "--runcard RUNCARD", "RunCard name (default: './runcards/stringSpinSim.card').") do |r|
    if valid_path?(r)
      options[:run_card] = r
    else
      puts "Error: RunCard '#{r}' does not exist."
      exit
    end
  end

  opts.on("-c", "--events COUNT", Integer, "Number of events (default: 1000).") do |c|
    options[:events] = c
  end

  opts.on("-o", "--output-dir DIR", "Output directory (default: './out/').") do |o|
    if valid_path?(o)
      options[:output_dir] = o
    else
      puts "Error: Output directory '#{o}' does not exist."
      exit
    end
  end

  opts.on("-p", "--process-macros MACROS", "Process macros.") do |p|
    options[:process_macros] = p
  end

  opts.on("-b", "--num-batches BATCHES", Integer, "Number of batches for parallel processing (default: 1).") do |b|
    options[:num_batches] = b
  end

  opts.on_tail("-h", "--help", "Show this message") do
    puts opts
    puts "\nMandatory parameters:"
    puts "  -n, --name NAME: Project name (cannot be empty)."
    puts "  -e, --executable EXECUTABLE: Executable name (must exist in './pythia_programs')."
    puts "\nDefault values:"
    puts "  -r, --runcard RUNCARD: './runcards/stringSpinSim.card'"
    puts "  -c, --events COUNT: 1000"
    puts "  -o, --output-dir DIR: './out/'"
    puts "  -b, --num-batches BATCHES: 1"
    puts "\nAvailable executables in './pythia_programs':"
    Dir.foreach('./pythia_programs') do |file|
      puts "  #{file}" if File.file?("./pythia_programs/#{file}")
    end
    exit
  end
end.parse!

# Check mandatory options
unless options[:project_name]
  puts "Error: Project name (-n) is mandatory."
  exit
end

unless options[:executable_name]
  puts "Error: Executable (-e) is mandatory."
  exit
end


project_dir = "#{options[:output_dir]}/#{options[:project_name]}"
log_dir = "#{project_dir}/log"
slurm_dir = "#{project_dir}/slurm"

if Dir.exist?(project_dir)
  if options[:force]
    puts "The directory #{project_dir} will be overwritten due to the force option."
    FileUtils.rm_rf(project_dir)
    puts "The directory #{project_dir} has been overwritten due to the force option."
  else
    puts_lightred("The directory #{project_dir} already exists.")
    puts "Would you like to delete the existing directory and proceed? [Y/n]"
    answer = STDIN.gets.chomp.downcase
    if answer == 'y'
      FileUtils.rm_rf(project_dir)
      puts "The directory has been deleted."
    else
      puts "Operation canceled by the user."
      exit
    end
  end
end


FileUtils.mkdir_p(project_dir)
FileUtils.mkdir_p(log_dir)
FileUtils.mkdir_p(slurm_dir)

# Array to hold job IDs
job_ids = []

# Generate and submit a Slurm job for each batch
options[:num_batches].times do |batch_index|
  slurm_script = <<-SLURM
#!/bin/bash
#SBATCH --job-name=#{options[:project_name]}_#{batch_index}
#SBATCH --account=clas12
#SBATCH --partition=production
#SBATCH --mem-per-cpu=1000
#SBATCH --cpus-per-task=1
#SBATCH --output=#{log_dir}/#{options[:project_name]}_#{batch_index}.out
#SBATCH --error=#{log_dir}/#{options[:project_name]}_#{batch_index}.err
#SBATCH --time=24:00:00

# Execute the create_project.rb script with batch-specific parameters
ruby create_project.rb -n #{options[:project_name]} -e #{options[:executable_name]} -r #{options[:run_card]} -c #{options[:events]} -f -b #{batch_index} #{options[:process_macros] ? "-p #{options[:process_macros]}" : ''}

  SLURM

  # Save the Slurm script to a file
  script_path = "#{slurm_dir}/batch_#{batch_index}_#{options[:project_name]}.slurm"
  File.write(script_path, slurm_script)

  # Submit the Slurm job
  output = `sbatch #{script_path}`
  if output =~ /Submitted batch job (\d+)/
    job_ids << $1
  end
end

puts "Submitted #{options[:num_batches]} batch jobs for project #{options[:project_name]}."


# Create a dependency string for the job IDs
dependency_str = job_ids.join(':')

# Submit a final job to run hadd after all other jobs have completed
final_slurm_script = <<-SLURM
#!/bin/bash
#SBATCH --dependency=afterok:#{dependency_str}
#SBATCH --job-name=#{options[:project_name]}_merge
#SBATCH --account=clas12
#SBATCH --partition=production
#SBATCH --mem-per-cpu=8000
#SBATCH --cpus-per-task=1
#SBATCH --output=#{log_dir}/#{options[:project_name]}_merge.out
#SBATCH --error=#{log_dir}/#{options[:project_name]}_merge.err
#SBATCH --time=01:00:00

# Command to merge ROOT files
hadd -f #{project_dir}/analysis.root #{project_dir}/batch*.root

# Remove all .root files apart from the first one, just to save it
find #{project_dir} -type f -name "batch*.root" | grep -vE "^#{project_dir}/batch0_.*\.root$" | xargs rm -f


SLURM

# Save the final Slurm script to a file
final_script_path = "#{slurm_dir}/#{options[:project_name]}_merge.slurm"
File.write(final_script_path, final_slurm_script)

# Submit the final Slurm job
system("sbatch #{final_script_path}")

puts "Submitted final merge job for project #{options[:project_name]}."