public class Main {

    public static void main(String[] args) {
	// write your code here
        int[] arr = new int[10];
        for (int i = 0; i < arr.length; i++) {
            arr[i] = 10;
        }

        int[] scores = new int[]{90, 88, 100};

        for (int score : scores) {
            System.out.println(score);
        }

        System.out.println(arr[9]);
    }
}
