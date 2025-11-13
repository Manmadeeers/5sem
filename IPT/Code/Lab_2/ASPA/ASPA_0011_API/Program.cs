using ASPA_0011_API.Services;
using Serilog;

//Important to include Serilog.Sinks.File and Serilog.AspNetCore and Serilog as well
internal class Program
{
    private static void Main(string[] args)
    {


        var builder = WebApplication.CreateBuilder(args);

        var contentRoot = Directory.GetCurrentDirectory();

        var debugFolder = Path.Combine(contentRoot, "Debug");

        var logFilePath = Path.Combine(debugFolder, "app-.log");


        Log.Logger = new LoggerConfiguration()
            .MinimumLevel.Debug()
            .Enrich.FromLogContext()
            .WriteTo.File(
                path: logFilePath
            )
            .CreateLogger();

        try
        {
            Log.Information("Started");

            builder.Host.UseSerilog();
            

            // Add services to the container.

            builder.Services.AddControllers();
            // Learn more about configuring Swagger/OpenAPI at https://aka.ms/aspnetcore/swashbuckle
            builder.Services.AddEndpointsApiExplorer();
            builder.Services.AddSwaggerGen();



            builder.Services.AddSingleton<ChannelService>();




            var app = builder.Build();

            try

            {

                if (app.Environment.IsDevelopment())

                {

                    app.UseDeveloperExceptionPage();

                    app.UseSwagger();

                    app.UseSwaggerUI(c => c.SwaggerEndpoint("/swagger/v1/swagger.json", "v1"));

                }

                app.UseHttpsRedirection();

                app.UseAuthorization();

                app.MapControllers();

                app.Run();

            }

            catch (Exception ex)

            {

                var logger = app.Services.GetRequiredService<ILogger<Program>>();

                logger.LogCritical(ex, "Host terminated unexpectedly");

                throw;

            }
        }
        catch (Exception ex)
        {
            Log.Fatal(ex, "Host was terminated unexpectedly");
            throw;
        }
        finally
        {
            Log.CloseAndFlush();
        }
        
    }
}