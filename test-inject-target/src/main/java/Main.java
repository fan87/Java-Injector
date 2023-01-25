public class Main {

    public static void main(String[] args) throws InterruptedException {
//        System.out.println("PID: " + ProcessHandle.current().pid());
        while (true) {
            System.out.println("Waiting...");
            Thread.sleep(1000);
        }
    }

}