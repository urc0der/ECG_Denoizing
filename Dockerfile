# Use the official Python image as the base image
FROM python:3.12-slim

# Set the working directory in the container
WORKDIR /app

# Copy the application code into the container
COPY . /app

# Install the required Python packages
RUN pip install --no-cache-dir -r requirements.txt

# Expose the port (default for local dev, Render overrides with $PORT)
EXPOSE 5000

# Run the app with Gunicorn in production
CMD gunicorn -b 0.0.0.0:$PORT app:app
