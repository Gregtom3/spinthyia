#!/usr/bin/env ruby
require 'optparse'
require 'fileutils'
require 'time'

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
  project_name: nil,
  executable_name: nil,
  run_card: nil,
  events: nil,
  output_dir: "#{Dir.pwd}/out",
  prefix: "",
  force: false,
  process_macros: nil
}

OptionParser.new do |opts|
  opts.banner = "Usage: create_project.rb [options]"

  opts.on("-n", "--name NAME", "Project name. Names the directory containing output (ex: -n my_project).") { |n| options[:project_name] = n }
  opts.on("-e", "--executable EXECUTABLE", "Executable program name, located in ./bin (ex: -e dis).") { |e| options[:executable_name] = e }
  opts.on("-r", "--runcard RUNCARD", "RunCard name, located in ./runcards (ex: -r my_runcard.cmnd).") { |r| options[:run_card] = r }
  opts.on("-c", "--events COUNT", Integer, "Number of events for the simulation (ex: -c 1000 ).") { |c| options[:events] = c }
  opts.on("-o", "--output-dir DIR", "Output directory (defaults to ./out)") { |o| options[:output_dir] = o }
  opts.on("-p", "--prefix PREFIX", "Prefix for files (default is no prefix)") { |p| options[:prefix] = p }
  opts.on("-f", "--force", "Automatically append to the output directory without prompt") { options[:force] = true }
  opts.on("-p", "--process-macros MACROS", "Comma-separated list of process macros to run after the executable (ex: -p macro1.C,macro2.C)") do |m|
    options[:process_macros] = m.split(',')
  end
  opts.on_tail("-h", "--help", "Show this message") do
    puts opts
    exit
  end
end.parse!


# Catch the case where an executable is not named
if !options[:executable_name]
    puts_lightred("ERROR: The parameter :executable: must be defined...Run ./create_project.rb --help for info...Aborting...")
    exit
end

# Catch the case where a project_name is not given
if !options[:project_name]
    puts_lightred("WARNING: The parameter :project_name: is typically required...defaulting to :project_name:=tmp")
    options[:project_name] = "tmp"
end

project_dir = "#{options[:output_dir]}/#{options[:project_name]}"
runcard_dir = "#{project_dir}/runcards"
gen_out_dir = "#{project_dir}/gen"

if Dir.exist?(project_dir) && !options[:force]
  puts_lightred("The directory #{project_dir} already exists.")
  puts "Would you like to proceed and use the existing directory? [Y/n]"
  answer = STDIN.gets.chomp.downcase
  exit unless answer == 'y'
end

# Create directories
FileUtils.mkdir_p(runcard_dir)
FileUtils.mkdir_p(gen_out_dir)

# Copy the runCard with optional prefix
prefix = options[:prefix].empty? ? "" : "#{options[:prefix]}_"

if options[:run_card]
    run_card_dest = "#{runcard_dir}/#{prefix}#{options[:run_card]}"
    FileUtils.cp("#{Dir.pwd}/runcards/#{options[:run_card]}", run_card_dest)
end

puts_lightgreen("Project #{options[:project_name]} created successfully.")

# Depending on the provided options, and required_params of the executable, throw errors
def check_missing_parameters(required_params, options)
  missing_params = required_params.select { |param| options[param].nil? || options[param].to_s.empty? }
  unless missing_params.empty?
    missing_params.each do |param|
      puts "Missing parameter for this operation: #{param}"
    end
    exit
  end
end
    
# Run the executable
executable_name = options[:executable_name]

case executable_name
  when "dis"
    puts_lightblue("Running 'dis'")
    executable_line = "./bin/dis"
  when "pythia8_to_gemc_lund"
    gen_out_dir_v2 = "#{gen_out_dir}/pythia8"
    required_params = [:events, :project_name, :run_card]
    check_missing_parameters(required_params, options)
    puts_lightblue("Running 'pythia8_to_gemc_lund'")
    FileUtils.mkdir_p(gen_out_dir_v2)
    executable_line = "./bin/pythia8_to_gemc_lund #{gen_out_dir_v2} #{runcard_dir}/#{options[:run_card]} #{options[:events]/4} 0 #{rand(1000000)}\n"
    executable_line += "./bin/pythia8_to_gemc_lund #{gen_out_dir_v2} #{runcard_dir}/#{options[:run_card]} #{options[:events]/4} 1 #{rand(1000000)}\n"
    executable_line += "./bin/pythia8_to_gemc_lund #{gen_out_dir_v2} #{runcard_dir}/#{options[:run_card]} #{options[:events]/4} 2 #{rand(1000000)}\n"
    executable_line += "./bin/pythia8_to_gemc_lund #{gen_out_dir_v2} #{runcard_dir}/#{options[:run_card]} #{options[:events]/4} 3 #{rand(1000000)}"
  when "clasdis"
    gen_out_dir_v2 = "#{gen_out_dir}/clasdis"
    required_params = [:events, :run_card]
    check_missing_parameters(required_params, options)
    puts_lightblue("Running 'clasdis'")
    runcard_path = "#{runcard_dir}/#{options[:run_card]}"
    # Build the command line from the runcard
    runcard_options = File.readlines(runcard_path).map do |line|
      # Split line at the first occurrence of "#" and take the first part
      option_part = line.split("#", 2).first.strip
      # Skip if the option_part is empty (i.e., the line was a comment or empty)
      next if option_part.empty?
      option_part
    end.compact.join(" ") 
    FileUtils.mkdir_p(gen_out_dir_v2)
    executable_line = "./deps/clasdis/clasdis #{runcard_options} --trig #{options[:events]} --path #{gen_out_dir_v2}"
  else
    puts "Executable '#{executable_name}' is not recognized."
    exit
end

# Run the executable with the desired options
run_executable(executable_line)

# Print out completion
puts_lightgreen("\n\nExecutable Finished.\n")

def run_root_macros(macros, gen_out_dir_v2)
  macros.each do |macro|
    macro_path = "#{Dir.pwd}/macros/#{macro}"
    unless File.exist?(macro_path)
      puts_lightred("Macro #{macro} not found at #{macro_path}. Skipping...")
      next
    end
    puts_lightblue("Running ROOT macro: #{macro}")
    macro_filename_without_extension = File.basename(macro, File.extname(macro))
    output_filename = "analysis_#{macro_filename_without_extension}.root"
    system("root -l -b -q '#{macro_path}(\"#{gen_out_dir_v2}\",\"#{output_filename}\")'")
  end
end

# If process macros are specified, run them
if options[:process_macros]
  puts_lightgreen("\nRunning process macros...")
  run_root_macros(options[:process_macros], gen_out_dir_v2)
end

# Print out completion
puts_lightgreen("\n\nProgram finished. Exiting.\n")

